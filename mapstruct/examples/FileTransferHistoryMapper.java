/*
 * VTB Group. Do not reproduce without permission in writing.
 * Copyright (c) 2021 VTB Group. All rights reserved.
 */

package ****.mapper;

import ****.integration.artemis.dto.dbo.MessageFromDboDto;
import ****.integration.kafka.consumer.dto.FromBankMessageDto;
import ****.integration.kafka.consumer.dto.PacketIncomingMessageDto;
import ****.model.EcmFileWrapper;
import ****.persistence.FileTransferHistory;
import ru.vtb.svpc.ecmwrapper.api.dto.esign.ESignResponseWrapper;
import ru.vtb.svpc.ecmwrapper.api.dto.file.FileInfoDto;

import org.mapstruct.InjectionStrategy;
import org.mapstruct.Mapper;
import org.mapstruct.Mapping;
import org.mapstruct.Named;

import java.util.UUID;

@Named("FileTransferHistoryMapper")
@Mapper(
        componentModel = "spring",
        uses = FileTransferHistorySignatureMapper.class,
        injectionStrategy = InjectionStrategy.CONSTRUCTOR
)
public interface FileTransferHistoryMapper {

    @Mapping(target = "id", ignore = true)
    @Mapping(target = "fileTransferHistorySignatureSet", ignore = true)
    @Mapping(target = "documentId", source = "dto.document.documentId")
    @Mapping(target = "documentType", source = "dto.document.documentType")
    @Mapping(target = "fileName", source = "dto.file.fileName")
    @Mapping(target = "fileUuid", source = "dto.file.uuid")
    @Mapping(target = "direction", constant = "TO_BANK")
    @Mapping(target = "createDateTime", ignore = true)
    FileTransferHistory map(MessageFromDboDto dto);

    @Mapping(target = "id", ignore = true)
    @Mapping(target = "fileTransferHistorySignatureSet", ignore = true)
    @Mapping(target = "documentId", source = "dto.documentMeta.documentId")
    @Mapping(target = "documentType", source = "dto.documentMeta.documentType")
    @Mapping(target = "fileName", source = "fileName")
    @Mapping(target = "fileUuid", source = "fileUuid")
    @Mapping(target = "direction", constant = "FROM_BANK")
    @Mapping(target = "createDateTime", ignore = true)
    FileTransferHistory map(FromBankMessageDto dto, String fileName, UUID fileUuid);

    @Mapping(target = "id", ignore = true)
    @Mapping(target = "fileTransferHistorySignatureSet", ignore = true)
    @Mapping(target = "documentId", source = "dto.documentMeta.documentId")
    @Mapping(target = "documentType", source = "dto.documentMeta.documentType")
    @Mapping(target = "fileName", source = "dto.archiveFileName")
    @Mapping(target = "fileUuid", source = "dto.archiveUuid")
    @Mapping(target = "direction", constant = "TO_BANK")
    @Mapping(target = "createDateTime", ignore = true)
    FileTransferHistory map(PacketIncomingMessageDto dto);

    @Deprecated
    @Mapping(target = "id", ignore = true)
    @Mapping(target = "fileTransferHistorySignatureSet", ignore = true)
    @Mapping(target = "documentId", source = "documentId")
    @Mapping(target = "documentType", source = "documentType")
    @Mapping(target = "fileName", source = "dto.data.name")
    @Mapping(target = "fileUuid", source = "dto.data.uuid")
    @Mapping(target = "direction", constant = "FROM_BANK")
    @Mapping(target = "createDateTime", ignore = true)
    FileTransferHistory map(ESignResponseWrapper<FileInfoDto> dto, Long documentId, Integer documentType);

    @Mapping(target = "id", ignore = true)
    @Mapping(target = "fileTransferHistorySignatureSet", ignore = true)
    @Mapping(target = "documentId", source = "documentId")
    @Mapping(target = "documentType", source = "documentType")
    @Mapping(target = "fileName", source = "dto.name")
    @Mapping(target = "fileUuid", source = "dto.uuid")
    @Mapping(target = "direction", constant = "FROM_BANK")
    @Mapping(target = "createDateTime", ignore = true)
    FileTransferHistory map(FileInfoDto dto, Long documentId, Integer documentType);

    @Mapping(target = "id", ignore = true)
    @Mapping(target = "fileTransferHistorySignatureSet", ignore = true)
    @Mapping(target = "documentId", ignore = true)
    @Mapping(target = "documentType", ignore = true)
    @Mapping(target = "fileName", source = "fileName")
    @Mapping(target = "fileUuid", source = "dto.uuid")
    @Mapping(target = "direction", constant = "TO_BANK")
    @Mapping(target = "createDateTime", ignore = true)
    FileTransferHistory toBankMap(EcmFileWrapper dto, String fileName);

}
